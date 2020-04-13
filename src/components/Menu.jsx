import React from 'react'
import { css } from 'glamor'
import { useSelector } from 'react-redux'
import { BrainChart, Check, Ratings, Plus } from './Icons'

const links = [
	{
		url: '/',
		label: 'Findings',
		icon: BrainChart,
	},
	{
		url: '/results',
		label: 'Fortunes',
		icon: Ratings,
	},
	{
		url: '/vote',
		label: 'Approve',
		icon: Check,
	},
	{
		url: '/add',
		label: 'Create',
		icon: Plus,
	},
]

export default () => {
	const theme = useSelector(state => state.theme) || []

	return (
		<section
			{...css({
				zIndex: '99',
				position: 'fixed',
				bottom: 0,
				left: 0,
				width: '100%',
				padding: '1rem 0 2rem 0',
				fontSize: '1.25rem',
				[`@media(min-width: ${theme.breaks.menu}px)`]: {
					fontSize: '1.5rem',
					padding: '1rem 0',
				},
				backgroundColor: '#000000',
				borderTop: `.25rem solid rgba(${theme.backgroundColor}, .7)`,
			})}>
			<ul {...css({ listStyle: 'none', margin: 0, padding: 0, textAlign: 'center' })}>
				{links &&
					links.map((link, i) => (
						<li
							key={link.url}
							{...css({
								width: '24%',
								display: 'inline-block',
								// paddingLeft: '1.5rem',
								// marginLeft: '1.5rem',
								borderLeft: '1px solid rgba(255, 255, 255, .2)',
								'&:nth-child(1)': {
									paddingLeft: 0,
									marginLeft: 0,
									borderLeft: 'none',
								},
							})}>
							<a
								href={link.url}
								{...css({
									textDecoration: 'none',
									textTransform: 'uppercase',
									textAlign: 'center',
									tapHighlightColor: '#FFF',
									color: theme.background,
									[`@media(min-width: ${theme.breaks.menu}px)`]: {
										display: 'flex',
										alignItems: 'center',
										justifyContent: 'center',
									},
									'&:active': {
										color: '#FFF',
										'& img': {
											filter: 'invert(50%)',
										},
									},
									'@media (hover: hover) and (pointer: fine)': {
										'&:hover': {
											color: '#FFF',
											'& img': {
												filter: 'invert(50%)',
											},
										},
										'&:active': {
											color: '#FFF',
											'& img': {
												filter: 'invert(50%)',
											},
										},
									},
								})}>
								<link.icon
									css={{
										display: 'block',
										margin: '0 auto .25rem auto',
										height: '1.75rem',
										maxWidth: '3rem',
										'& path': {
											fill: theme.background,
										},
										[`@media(min-width: ${theme.breaks.menu}px)`]: {
											display: 'inline-block',
											margin: '0 .5rem 0 0',
											maxWidth: '4rem',
										},
									}}
								/>

								<span>{link.label}</span>
							</a>
						</li>
					))}
			</ul>
		</section>
	)
}
